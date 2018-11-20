import { CMapObject } from './types';

const checkArgs = (...args) => {
  for (let i = 0; i < args.length; ++i) {
    if (!(args[i] instanceof CMapObject)) {
      throw new Error(`Argument isn\'t instance of CMapObject : ${args[i]}`);
    }
  }
};

const getCFuncIdent = (wasmModule, ident, ...args) => {
  for (let i = 0; i < args.length; ++i) {
    ident += '_' + args[i].type().ident();
  }
  return ident;
};

/**
 * Call
 * @param {object} wasmModule
 * @param {string} ident
 * @param {class} returnTypeClass
 * @param {CMapObject} ...args
 */
const callCFunction = (
  wasmModule,
  writeBackHints,
  ident,
  returnTypeClass,
  isReturnRef,
  ...args
) => {
  checkArgs(...args);

  const func = wasmModule[ident];
  // assert(func, 'Function not found: ' + ident);

  const cArgs = [];
  for (let i = 0; i < args.length; ++i) {
    cArgs[i] = args[i].readToMem(wasmModule);
  }

  const retAddr = func.apply(null, cArgs);
  let ret = null;
  if (returnTypeClass) {
  	let data;
  	if (isReturnRef) {
	    data = CMapObject.createBufferViewFromMem(
	      wasmModule.HEAP8,
	      retAddr,
	      returnTypeClass._type.size()
	    );
	  } else {
	    const end = retAddr + returnTypeClass._type.size();
	    data = wasmModule.HEAP8.slice(retAddr, end);
	  }
	  ret = new returnTypeClass(data);
  }

  for (let i = 0; i < args.length; ++i) {
    if (writeBackHints[i]) {
      args[i].writeBackFromMem(wasmModule, cArgs[i]);
    }
    args[i].freeFromMem(wasmModule, cArgs[i]);
  }
  return ret;
};

export { getCFuncIdent, callCFunction };
