import { connect } from 'react-redux';
import HomePage from '../components/HomePage/HomePage';
import LogAction from '../actions/log';

const mapStateToProps = (state, ownProps) => ({
  ...ownProps,
  ...state.logs,
});

const mapDispatchToProps = {
  getLogs: LogAction.getLogs,
};

export default connect(mapStateToProps, mapDispatchToProps)(HomePage);