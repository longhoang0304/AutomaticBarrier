import React, { Component } from 'react';
import LogList from '../LogList';
import './HomePage.css';

export default class HomePage extends Component {
  componentDidMount() {
    const { getLogs } = this.props;
    getLogs();
    this.getLogInterval = setInterval(() => getLogs(), 1000);
  }

  componentWillUnmount() {
    clearInterval(this.getLogInterval);
  }

  render() {
    const { logList } = this.props;

    return (
      <div id="homepage">
        <LogList data={logList} />
      </div>
    )
  }
}
