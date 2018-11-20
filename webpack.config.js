const path = require('path');
const webpack = require('webpack');

const generateConfig = (
  entry,
  outputName,
  includeFrom,
  resolveAlias,
  envNames
) => ({
  node: {
    __dirname: true,
    fs: 'empty',
  },
  target: 'web',
  watchOptions: {
    ignored: '/node_modules/',
    poll: true
  },
  entry: [entry],
  output: {
    path: path.resolve(__dirname, 'build/bundle'),
    filename: outputName,
  },
  mode: 'development',
  resolve: {
    extensions: ['.js', '.jsx'],
    modules: [path.resolve(__dirname, 'node_modules')],
    alias: Object.keys(resolveAlias).reduce(function(previous, key) {
      previous[key] = path.resolve(__dirname, resolveAlias[key]);
      return previous;
    }, {})
  },
  devtool: 'source-map',
  plugins: [new webpack.EnvironmentPlugin(envNames)],
  module: {
    rules: [
      {
        test: /\.jsx?$/,
        include: path.resolve(__dirname, includeFrom),
        use: {
          loader: 'babel-loader',
          options: {
            presets: ['babel-preset-env'],
            plugins: [
              require('babel-plugin-transform-runtime'),
            ]
          }
        }
      },
      {
        test: /\.node$/,
        use: 'node-loader'
      }
    ]
  },
  devServer: {
    contentBase: path.join(__dirname, 'build'),
    watchContentBase: true,
  }
});

module.exports = [
  generateConfig(
    './src/main.js',
    'build.js',
    'src',
    {
      wasm: './build/bundle',
    },
    []
  )
];
