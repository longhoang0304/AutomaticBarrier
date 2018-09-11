import { LogTypes } from '../constants/ActionTypes';
import { get, APIUrl, del } from '../lib/helper';

const fetchingLogs = () => ({
  type: LogTypes.LOG_GET_ALL,
});

const getLogsSuccess = (logList) => ({
  type: LogTypes.LOG_GET_ALL_SUCCESS,
  payload: {
    logList,
  },
});

const getLogsFailed = (errorMsg) => ({
  type: LogTypes.LOG_GET_ALL_FAILED,
  payload: {
    errorMsg,
  },
});


const getLogs = () => async (dispatch) => {
  dispatch(fetchingLogs());
  let res;
  try {
    res = await get(APIUrl('logs'));
    const json = await res.json();
    if (res.ok) {
      dispatch(getLogsSuccess(json));
      return;
    }
    const { message } = json;
    dispatch(getLogsFailed(message || 'Unknown Error Occurred'));
  } catch (error) {
    console.log(error.message);
    dispatch(getLogsFailed(error.message));
  }
};

const archivingLogs = () => ({
  type: LogTypes.LOG_ARCHIVE,
});

const archivedLogSuccess = (index) => ({
  type: LogTypes.LOG_ARCHIVE_SUCCESS,
  payload: {
    index,
  },
});

const archivedLogFailed = (errorMsg) => ({
  type: LogTypes.LOG_ARCHIVE_FAILED,
  payload: {
    errorMsg,
  },
});


const archiveLog = (index) => async (dispatch, getStore) => {
  dispatch(archivingLogs());
  let res;
  const store = getStore().logs;
  const { logList } = store;
  try {
    res = await del(APIUrl(`logs/${logList[index]._id}`));
    const json = await res.json();
    if (res.ok) {
      dispatch(archivedLogSuccess(json));
      return;
    }
    const { message } = json;
    dispatch(archivedLogFailed(message || 'Unknown Error Occurred'));
  } catch (error) {
    console.log(error.message);
    dispatch(archivedLogFailed(error.message));
  }
};

const LogsActions = {
  getLogs,
  archiveLog
};

export default LogsActions;