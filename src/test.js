import { CLong, CFloat, CDouble, createStruct, CUnsignedLong } from './types';
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
      _Point_disToOrigin: [[1], CDouble, 0],
      _distance_Point_Point: [[0, 0], CDouble, 0],
      _debug_i32: [[0], null, 0],
      _debug_f64: [[0], null, 0],
    };
    C.add = (...args) => {
      const ident = getCFuncIdent(wasmModule, '_add', args);
      const hint = hints[ident];
      return callCFunction(
        wasmModule,
        hint[0],
        ident,
        hint[1],
        hint[2],
        args,
        null
      );
    };
    C.debug = (...args) => {
      const ident = getCFuncIdent(wasmModule, '_debug', args);
      const hint = hints[ident];
      return callCFunction(
        wasmModule,
        hint[0],
        ident,
        hint[1],
        hint[2],
        args,
        null
      );
    };
    C.Point = createStruct('Point', {
        'x':{type:CLong,offset:0},
        'y':{type:CLong,offset:4}
      }, 8, null, wasmModule, {
        _construct_Point: [[1], null, 0],
        _construct_Point_i32_i32: [[1,0, 0], null, 0]
      });
    C.Point.prototype.disToOrigin = function() {
      const ident = getCFuncIdent(wasmModule, '_Point_disToOrigin', arguments);
      const hint = hints[ident];
      const args = [this, ...arguments];
      return callCFunction(
        wasmModule,
        hint[0],
        ident,
        hint[1],
        hint[2],
        args,
        null
      );
    }
    C.distance = (...args) => {
      const ident = getCFuncIdent(wasmModule, '_distance', args);
      const hint = hints[ident];
      return callCFunction(
        wasmModule,
        hint[0],
        ident,
        hint[1],
        hint[2],
        args,
        null
      );
    };


    this.C = C;
  }
}

export { TestModule };
