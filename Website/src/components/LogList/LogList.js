import React, { Component } from 'react';
import LogItem from '../../containers/LogItem';
import LogHeader from '../LogHeader';
import _ from 'lodash';
import './style.css';

export default class LogList extends Component {
  render() {
    const { data } = this.props;
    return (
      <div id="log-list">
        <LogHeader />
        {_.map(data, (e, i) => <LogItem item={e} key={i} no={i} />)}
      </div>
    )
  }
}
