import { CLong, CFloat, CDouble } from './types';
import { callCFunction, getCFuncIdent } from './wrap';
import { Module } from './module';

class TestModule extends Module {
  initCMethod() {
    const wasmModule = this._wasmModule;
    const C = {};

    const hints = {
      _add_i32: [[1], CLong, 0],
      _add_f32: [[1], CFloat, 0],
      _add_f64: [[1], CDouble, 0],
      _printf_i32: [[], CLong, 0],
      _debug: [[], null, 0],
    };
    C.add = (...args) => {
      const ident = getCFuncIdent(wasmModule, '_add', ...args);
      const hint = hints[ident];
      return callCFunction(
        wasmModule,
        hint[0],
        ident,
        hint[1],
        hint[2],
        ...args
      );
    };
    C.printf = (...args) => {
      const ident = '_printf_i32';
      const hint = hints[ident];
      return callCFunction(
        wasmModule,
        hint[0],
        ident,
        hint[1],
        hint[2],
        ...args
      );
    };
    C.printf = (...args) => {
      const ident = '_debug';
      const hint = hints[ident];
      return callCFunction(
        wasmModule,
        hint[0],
        ident,
        hint[1],
        hint[2],
        ...args
      );
    };

    this.C = C;
  }
}

export { TestModule };
