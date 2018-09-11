
import React, { Component } from 'react';
import './style.css';

export default class LogHeader extends Component {
  render() {
    return (
      <div id="log-header" className="log-item">
        <div>
          NO
        </div>
        <div>
          TRAIN CODE
        </div>
        <div>
          ROUTE
        </div>
        <div>
          STATION
        </div>
        <div>
          P. SPEED (Km/H)
        </div>
        <div>
          SPEED (Km/H)
        </div>
        <div>
          RECORDED AT
        </div>
        <div>
          STATUS
        </div>
        <div>
          ARCHIVE
        </div>
      </div>
    )
  }
}
