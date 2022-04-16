focument = {
    ready: function (x) {
        this.exec = x;
    }
};

var $ = function (x) {
    return document;
};

(function ($) {
    eval("(function(){\n" +
         "  var Private={};\n" +
         "  $(document).ready(function(){\n" +
         "      init()\n" +
         "  });\n" +
         "  function init(){\n" +
         "      $(Private)\n" +
         "  };\n" +
         "})();");
})($);
document.exec();

// Don't crash or assert.

