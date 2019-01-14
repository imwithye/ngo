const ngo = require('./src')

lib = ngo.openlib('ngo.so')
console.log(lib.call('ToLower', 'THIS SHOULD PRINT IN LOWER CASE.'))