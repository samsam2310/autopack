import { TestModule } from './test';
import Module from 'wasm/wasm';
import { createCStr, CLong, CFloat, CDouble } from './types';

console.log('GGGG111');

const mod = new TestModule(Module, {
  locateFile: f => {
    return '/bundle/' + f;
  }
});

const fmt = createCStr('Hello, %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n');
console.log(fmt);
const a = new CLong;

mod.then(()=>{
	console.log('Hi -----------------');
	mod.C.add(a);
	mod.C.add(a);
	mod.C.add(a);
	mod.C.add(a);
	mod.C.printf(fmt, a);
	console.log('Bye ------------------');
});

console.log("init end");