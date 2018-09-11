import { LogTypes } from '../constants/ActionTypes';

const initState = {
  errorMsg: '',
  isLoading: false,
  logList: [],
};

const logReducer = (state = initState, action) => {
  const { errorMsg, logList, index } = action.payload || {};
  switch (action.type) {
    case LogTypes.LOG_ARCHIVE:
    case LogTypes.LOG_GET_ALL: {
      return {
        ...state,
        isLoading: true,
        errorMsg: '',
      };
    }
    case LogTypes.LOG_GET_ALL_SUCCESS: {
      return {
        ...state,
        isLoading: false,
        errorMsg: '',
        logList,
      };
    }
    case LogTypes.LOG_ARCHIVE_FAILED:
    case LogTypes.LOG_GET_ALL_FAILED: {
      return {
        ...state,
        isLoading: false,
        errorMsg,
      };
    }
    case LogTypes.LOG_ARCHIVE_SUCCESS: {
      return {
        ...state,
        isLoading: false,
        logList: [...state.logList.slice(0, index), ...state.logList.slice(index + 1)],
      };
    }
    default: {
      return state;
    }
  }
};

export default logReducer;