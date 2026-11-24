"""Exercise the actual configure guard bodies without invoking the toolchain."""

import ast
from pathlib import Path
from types import SimpleNamespace
import unittest


ROOT = Path(__file__).resolve().parents[3]


def load_guard(name):
    source = ROOT / 'build' / 'moz.configure' / 'rust.configure'
    tree = ast.parse(source.read_text(encoding='utf-8'))
    function = next(node for node in tree.body if isinstance(node, ast.FunctionDef) and node.name == name)
    function.decorator_list = []
    module = ast.Module(body=[function], type_ignores=[])

    def die(message):
        raise ValueError(message)

    namespace = {'die': die}
    exec(compile(ast.fix_missing_locations(module), str(source), 'exec'), namespace)
    return namespace[name]


class XP64ConfigGuardTests(unittest.TestCase):
    def test_enabled_mode_rejects_other_targets_and_compilers(self):
        guard = load_guard('mypal_xp64_startup')
        for cpu, kernel, compiler in [('x86', 'WINNT', 'clang-cl'),
                                      ('x86_64', 'Linux', 'clang-cl'),
                                      ('x86_64', 'WINNT', 'gcc')]:
            target = SimpleNamespace(cpu=cpu, kernel=kernel)
            toolchain = SimpleNamespace(type=compiler)
            with self.subTest(cpu=cpu, kernel=kernel, compiler=compiler):
                self.assertIsNone(guard(False, target, toolchain))
                with self.assertRaisesRegex(ValueError, 'Windows x64 clang-cl'):
                    guard(True, target, toolchain)
        self.assertTrue(guard(True, SimpleNamespace(cpu='x86_64', kernel='WINNT'),
                              SimpleNamespace(type='clang-cl')))

    def test_custom_rust_target_cannot_escape_xp64_mode(self):
        guard = load_guard('mypal_xp64_rust_target')
        target = ('x86_64-xp-windows-msvc',)
        self.assertEqual(guard(target, True), target[0])
        self.assertIsNone(guard((), False))
        with self.assertRaises(ValueError):
            guard(target, False)
        with self.assertRaises(ValueError):
            guard(('i686-pc-windows-msvc',), True)


if __name__ == '__main__':
    unittest.main()
