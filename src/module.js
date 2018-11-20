class Module {
  constructor(wasmModuleConstructor, mixin = {}) {
    this._wasmModule = wasmModuleConstructor(mixin);
    this._readyPromise = new Promise((resolv, reject) => {
      this._wasmModule.then(() => {
        resolv();
      });
    });

    this._wasmModule.then(() => {
      this._ready = true;
    });
    this.C = {};
    this.initCMethod();
  }

  async ready() {
    await this._readyPromise;
  }

  then(func) {
    this._readyPromise.then(func);
  }

  initCMethod() {
    throw Error('No Implementation Error');
  }
}

export { Module };
