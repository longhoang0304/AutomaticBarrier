import { connect } from 'react-redux';
import LogItem from '../components/LogItem';
import LogAction from '../actions/log';

const mapStateToProps = (state, ownProps) => ({
  ...ownProps,
});

const mapDispatchToProps = {
  archiveLog: LogAction.archiveLog,
};

export default connect(mapStateToProps, mapDispatchToProps)(LogItem);