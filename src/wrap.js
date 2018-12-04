import { CMapObject } from './types';

const checkArgs = (args) => {
  for (let i = 0; i < args.length; ++i) {
    if (!(args[i] instanceof CMapObject)) {
      throw new Error(`Argument isn\'t instance of CMapObject : ${args[i]}`);
    }
  }
};

const getCFuncIdent = (wasmModule, ident, args) => {
  for (let i = 0; i < args.length; ++i) {
    let argIdent = args[i].type().ident();
    if (argIdent === 'arr') {
      argIdent = 'i32';
    }
    ident += '_' + argIdent;
  }
  return ident;
};

const genCArgs = (wasmModule, args, vaArgs) => {
  const cArgs = [];
  for (let i = 0; i < args.length; ++i) {
    cArgs[i] = args[i].readToMem(wasmModule);
  }
  return cArgs;
};

const genReturnValue = (wasmModule, retAddr, returnTypeClass, isReturnRef) => {
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
  return ret;
};

const doWriteBack = (wasmModule, args, cArgs, writeBackHints) => {
  for (let i = 0; i < args.length; ++i) {
    if (writeBackHints[i]) {
      args[i].writeBackFromMem(wasmModule, cArgs[i]);
    }
    args[i].freeFromMem(wasmModule, cArgs[i]);
  }
};

/**
 * Call
 * @param {object} wasmModule
 * @param {string} ident
 * @param {class} returnTypeClass
 * @param {CMapObject[]} args
 */
const callCFunction = (
  wasmModule,
  writeBackHints,
  ident,
  returnTypeClass,
  isReturnRef,
  args
) => {
  checkArgs(args);
  const func = wasmModule[ident];
  // assert(func, 'Function not found: ' + ident);
  const cArgs = genCArgs(wasmModule, args);
  const retAddr = func.apply(null, cArgs);
  const ret = genReturnValue(wasmModule, retAddr, returnTypeClass, isReturnRef);
  doWriteBack(wasmModule, args, cArgs, writeBackHints);
  return ret;
};

export { getCFuncIdent, callCFunction };
