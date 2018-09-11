import chalk from 'chalk';
import moment from 'moment';

const { log } = console;
const tag = (status) => `[${status.padEnd(8)} - ${moment().format('DD/MMM/YYYY hh:mm:ss')}]:`;

const Logger = {
  log,
  note(...msg) {
    log(chalk.white(tag('NOTE'), msg));
  },
  info(...msg) {
    log(chalk.cyan(tag('INFO'), msg));
  },
  success(...msg) {
    log(chalk.green(tag('SUCCESS'), msg));
  },
  warning(...msg) {
    log(chalk.yellow(tag('WARNING'), msg));
  },
  error(...msg) {
    log(chalk.red(tag('ERROR'), msg));
  },
};

export default Logger;