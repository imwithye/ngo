const ngo = require('..')
const path = require('path')

lib = ngo.openlib(path.join(__dirname, 'example.so'))
console.log(lib.invoke('ToLower', 'This is to lower'))