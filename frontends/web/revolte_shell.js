var pressFunc;

Module = {
  print: function(what) {
    console.log(what);
  },
  printErr: function(what) {
    console.error(what);
  },
  canvas: (function() {
    return document.getElementById('canvas');
  })(),
  onRuntimeInitialized: function() {
    pressFunc = Module.cwrap('webButton', '', ['number', 'number']);
  }
};

function down(button) {
  if (pressFunc) {
    pressFunc(button, 1);
  }
}

function up(button) {
  if (pressFunc) {
    pressFunc(button, 0);
  }
}
