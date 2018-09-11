import { createStore, applyMiddleware } from 'redux';
import thunkMiddleware from 'redux-thunk';
import rootReducer from './reducers';

const initialState = {};
// const thunkMiddleware = createThunkMiddleware();

export default function configureStore() {
  const store = createStore(
    rootReducer,
    initialState,
    applyMiddleware(thunkMiddleware),
  );
  return store;
}