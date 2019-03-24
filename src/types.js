import { callCFunction, getCFuncIdent } from './wrap';

class TypeObject {
  /**
   * Constructor
   * @param {string} ident
   * @param {number} size
   * @param {Object} fields - {name:{offset, type}...}
   */
  constructor(ident, size, fields) {
    this._ident = ident;
    this._size = size;
    this._fields = fields;
  }

  ident() {
    return this._ident;
  }

  size() {
    return this._size;
  }

  fields() {
    return this._fields;
  }
}

class CMapObject {
  /**
   * Constructor
   * @param {TypeObject} type
   * @param {DataView} data
   */
  constructor(data = null) {
    if (!this.constructor._type) {
      throw new Error('No implement error');
    }
    this._data = data;
    if (!this._data) {
      const buf = new ArrayBuffer(this.type().size());
      this._data = new Int8Array(buf);
    }
  }

  type() {
    return this.constructor._type;
  }

  static _memCopy(srcArr, srcAddr, dstArr, dstAddr, len) {
    for (let i = 0; i < len; ++i) {
      dstArr[dstAddr + i] = srcArr[srcAddr + i];
    }
  }

  readToMem(wasmModule, addr = null) {
    const len = this.type().size();
    addr = addr || wasmModule['_malloc'](len);
    const mem = wasmModule.HEAP8;
    this.constructor._memCopy(this._data, 0, mem, addr, len);
    return addr;
  }

  freeFromMem(wasmModule, addr) {
    wasmModule['_free'](addr);
  }

  writeBackFromMem(wasmModule, addr) {
    const len = this.type().size();
    const mem = wasmModule.HEAP8;
    this.constructor._memCopy(mem, addr, this._data, 0, len);
  }

  static createBufferViewFromMem(typeArr, offset, len) {
    // assert(
    //   offset + len <= typeArr.length,
    //   'Cannot create TypeArray: out of range'
    // );
    return new Int8Array(typeArr.buffer, typeArr.offset + offset, len);
  }
}

class CBool extends CMapObject {}
CBool._type = new TypeObject('b', 1, {});

class CChar extends CMapObject {}
CChar._type = new TypeObject('i8', 1, {});

class CUnsignedChar extends CMapObject {}
CUnsignedChar._type = new TypeObject('u8', 1, {});

class CShort extends CMapObject {}
CShort._type = new TypeObject('i16', 2, {});

class CUnsignedShort extends CMapObject {}
CUnsignedShort._type = new TypeObject('u16', 2, {});

class CLong extends CMapObject {}
CLong._type = new TypeObject('i32', 4, {});

class CUnsignedLong extends CMapObject {}
CUnsignedLong._type = new TypeObject('u32', 4, {});

class CLongLong extends CMapObject {}
CLongLong._type = new TypeObject('i64', 8, {});

class CUnsignedLongLong extends CMapObject {}
CUnsignedLongLong._type = new TypeObject('u64', 8, {});

class CFloat extends CMapObject {}
CFloat._type = new TypeObject('f32', 4, {});

class CDouble extends CMapObject {}
CDouble._type = new TypeObject('f64', 8, {});

class CArray extends CMapObject {}

const createArrayType = (typeObj, len) => {
  const cls = class extends CArray {};
  cls._type = new TypeObject('arr', len * typeObj.size(), { type: typeObj });
  return cls;
};

const createCStr = str => {
  const data = new Int8Array(str.length + 1);
  for (var i = 0; i < str.length; ++i) {
    // assert((str.charCodeAt(i) === str.charCodeAt(i)) & 0xff);
    data[i] = str.charCodeAt(i) | 0;
  }
  const cls = createArrayType(CChar._type, str.length + 1);
  return new cls(data);
};

class CStruct extends CMapObject {
  constructor(...args) {
    if (args[0] instanceof Int8Array) {
      super(args[0]);
      return;
    }
    if (args.some(arg => !(arg instanceof CMapObject))) {
      console.log(args);
      throw new Error('Wrong argument type');
    }
    super();
    const ident = getCFuncIdent(
      this.constructor._wasmModule, '_construct_' + this.constructor._type.ident(), args);
    const hint = this.constructor._hints[ident];
    args.unshift(this);
    console.log('constr : ' + ident);
    callCFunction(this.constructor._wasmModule,
                  hint[0],
                  ident,
                  hint[1],
                  hint[2],
                  args);
  }
};

const getFieldFromStruct = (obj, ident) => {
  const field = obj.constructor._type.fields()[ident];
  const typeMapping = field.type;
  const data = new Int8Array(obj._data.buffer, field.offset, typeMapping._type.size());
  return new typeMapping(data);
}

const createStruct = (ident, fields, size, superClass, wasmModule, hints) => {
  const cls = superClass ? class extends superClass {} : class extends CStruct {};
  const defineField = (cls, fname) => {
    cls.prototype.__defineGetter__(fname, function() {
      return getFieldFromStruct(this, fname);
    });
  };
  for (let fname in fields) {
    defineField(cls, fname);
  }
  // size!?, end need pandding?
  cls._type = new TypeObject(ident, size, fields);
  cls._wasmModule = wasmModule;
  // unuse ?
  cls._hints = hints;
  return cls;
};

export {
  CMapObject,
  CBool,
  CChar,
  CShort,
  CLong,
  CLongLong,
  CUnsignedChar,
  CUnsignedShort,
  CUnsignedLong,
  CUnsignedLongLong,
  CFloat,
  CDouble,
  CArray,
  createArrayType,
  createCStr,
  createStruct,
};
