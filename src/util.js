"use strict";

module.exports = {
    validateInt: (...args) => args.forEach(num => {
        if (!Number.isInteger(num) || num < -0x80000000 || num > 0x7fffffff) {
            throw RangeError('Number is not a 32-bit signed integer');
        }
    }),
    
    validateString: str => {
        if (typeof str !== 'string' || !str.length || /[\u0100-\uffff\0]/g.test(str)) {
            throw TypeError("Invalid string.");
        }
    }
};