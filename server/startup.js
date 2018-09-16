/* eslint-disable no-underscore-dangle */
import log from './libs/logger';


/**
 * This function will generate admin account
 */

/**
 *
 * @param {Number | String} port
 * This function will start when server is ready
 */
async function startup(port) {
  log.log();
  log.success(`Listenning requests on port ${port}`);
  log.log();
}

export default startup;