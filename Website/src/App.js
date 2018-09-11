import React, { Component } from 'react';
import { Provider } from 'react-redux';
import configureStore from './configureStore';
import HomePage from './containers/HomePage';
import './App.css';

const store = configureStore();

class App extends Component {
  render() {
    return (
      <Provider store={store}>
        <HomePage />
      </Provider>
    );
  }
}

export default App;
