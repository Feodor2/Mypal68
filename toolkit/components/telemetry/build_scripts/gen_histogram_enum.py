# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this
# file, You can obtain one at http://mozilla.org/MPL/2.0/.

# Write out a C++ enum definition whose members are the names of
# histograms as well as the following other members:
#
#   - HistogramCount
#   - HistogramFirstUseCounter
#   - HistogramLastUseCounter
#   - HistogramUseCounterCount
#
# The histograms are defined in files provided as command-line arguments.

from __future__ import print_function
from mozparsers.shared_telemetry_utils import ParserError
from mozparsers import parse_histograms

import itertools
import sys
import buildconfig


banner = """/* This file is auto-generated, see gen_histogram_enum.py.  */
"""

header = """
#ifndef mozilla_TelemetryHistogramEnums_h
#define mozilla_TelemetryHistogramEnums_h

#include "mozilla/TemplateLib.h"
#include "mozilla/TypeTraits.h"

namespace mozilla {
namespace Telemetry {
"""

footer = """
} // namespace mozilla
} // namespace Telemetry
#endif // mozilla_TelemetryHistogramEnums_h"""


def main(output, *filenames):
    # Print header.
    print(banner, file=output)
    print(header, file=output)

    # Load the histograms.
    try:
        all_histograms = list(parse_histograms.from_files(filenames))
    except ParserError as ex:
        print("\nError processing histograms:\n" + str(ex) + "\n")
        sys.exit(1)

    groups = itertools.groupby(all_histograms,
                               lambda h: h.name().startswith("USE_COUNTER2_"))

    # Print the histogram enums.
    # Note that parse_histograms.py guarantees that all of the USE_COUNTER2_*
    # histograms are defined in a contiguous block.  We therefore assume
    # that there's at most one group for which use_counter_group is true.
    print("enum HistogramID : uint32_t {", file=output)
    seen_use_counters = False
    for (use_counter_group, histograms) in groups:
        if use_counter_group:
            seen_use_counters = True

        # The HistogramDUMMY* enum variables are used to make the computation
        # of Histogram{First,Last}UseCounter easier.  Otherwise, we'd have to
        # special case the first and last histogram in the group.
        if use_counter_group:
            print("  HistogramFirstUseCounter,", file=output)
            print("  HistogramDUMMY1 = HistogramFirstUseCounter - 1,", file=output)

        for histogram in histograms:
            if histogram.record_on_os(buildconfig.substs["OS_TARGET"]):
                print("  %s," % histogram.name(), file=output)

        if use_counter_group:
            print("  HistogramDUMMY2,", file=output)
            print("  HistogramLastUseCounter = HistogramDUMMY2 - 1,", file=output)

    print("  HistogramCount,", file=output)
    if seen_use_counters:
        print("  HistogramUseCounterCount = HistogramLastUseCounter -"
              " HistogramFirstUseCounter + 1", file=output)
    else:
        print("  HistogramFirstUseCounter = 0,", file=output)
        print("  HistogramLastUseCounter = 0,", file=output)
        print("  HistogramUseCounterCount = 0", file=output)
    print("};", file=output)

    # Write categorical label enums.
    categorical = filter(lambda h: h.kind() == "categorical", all_histograms)
    categorical = filter(lambda h: h.record_on_os(buildconfig.substs["OS_TARGET"]), categorical)
    enums = [("LABELS_" + h.name(), h.labels(), h.name()) for h in categorical]
    for name, labels, _ in enums:
        print("\nenum class %s : uint32_t {" % name, file=output)
        print("  %s" % ",\n  ".join(labels), file=output)
        print("};", file=output)

    print("\ntemplate<class T> struct IsCategoricalLabelEnum : std::false_type {};", file=output)
    for name, _, _ in enums:
        print("template<> struct IsCategoricalLabelEnum<%s> : std::true_type {};" % name,
              file=output)

    print("\ntemplate<class T> struct CategoricalLabelId {};", file=output)
    for name, _, id in enums:
        print("template<> struct CategoricalLabelId<%s> : "
              "std::integral_constant<uint32_t, %s> {};" % (name, id), file=output)

    # Footer.
    print(footer, file=output)


if __name__ == '__main__':
    main(sys.stdout, *sys.argv[1:])
