
import React, { Component } from 'react';
import m from 'moment';
import './style.css';

export default class LogItem extends Component {

  getStatusText(speed) {
    if(speed < 50) {
      return 'SLOW';
    }
    if(speed < 150) {
      return 'NORMAL';
    }
    if (speed < 220) {
      return 'WARNING';
    }
    return 'DANGEROUS';
  }

  render() {
    const { item, no, archiveLog } = this.props;

    const classList = "log-item " + this.getStatusText(item.speed).toLowerCase();

    return (
      <div className={classList}>
        <div>
          {no + 1}
        </div>
        <div>
          {item.trainCode || 'N/A'}
        </div>
        <div>
          {item.routeCode || 'N/A'}
        </div>
        <div>
          {item.station || 'N/A'}
        </div>
        <div>
          <span>80-120</span>
        </div>
        <div>
          {item.speed || 'N/A'}
        </div>
        <div>
          {item.createdAt ? m(item.createdAt).format("hh:mm:ss A, MMM DD YYYY") : 'N/A'}
        </div>
        <div>
          {this.getStatusText(item.speed)}
        </div>
        <div>
          <a href="javascript:void(0)" onClick={() => archiveLog(no)}>Archive</a>
        </div>
      </div>
    )
  }
}
