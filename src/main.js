import { TestModule } from './test';
import Module from 'wasm/wasm';
import { createCStr, CLong, CFloat, CDouble } from './types';

console.log('GGGG111');

const mod = new TestModule(Module, {
  locateFile: f => {
    return '/bundle/' + f;
  }
});

// const fmt = createCStr('Hello, %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n');
// console.log(fmt);
const a = new CLong;

// const vec = new MyVec(0, 1);
// vec.delete();

mod.then(()=>{
	console.log('Hi -----------------');
	mod.C.debug(mod.C.add(a));
	mod.C.add(a);
	mod.C.add(a);
	mod.C.add(a);
	mod.C.debug(a);
	console.log('1 ------------------');
	const pa = new mod.C.Point();
	const pb = new mod.C.Point(a, a);
	mod.C.add(pb.y);
	mod.C.debug(pa.x);
	mod.C.debug(pa.y);
	mod.C.debug(pb.x);
	mod.C.debug(pb.y);
	const res = pb.disToOrigin();
	console.log(res);
	mod.C.debug(res);
	console.log('2 ------------------');
	mod.C.debug(mod.C.distance(pa, pb));
	console.log('Bye ------------------');
});

console.log("init end");