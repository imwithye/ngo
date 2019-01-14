const ngo = require('./src')

lib = ngo.openlib('ngo.so')
console.log(lib.close())
console.log(lib.close())