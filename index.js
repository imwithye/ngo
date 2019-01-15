const ngo = require('./src')

lib = ngo.openlib('ngo.so')
console.log(lib.invoke('ToLower', 'This is to lower'))