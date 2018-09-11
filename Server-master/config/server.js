import express from 'express';
import bodyParser from 'body-parser';
import cookieParser from 'cookie-parser';
import helmet from 'helmet';
import cors from 'cors';
import morgan from 'morgan';
import chalk from 'chalk';
import routes from '../index.route';

const app = express();

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({
  extended: true,
}));

app.use(helmet());
app.use(cors());
app.use(cookieParser());
const getStatusColor = (status) => {
  switch (+status / 100) {
    case 2:
      return chalk.green;
    case 4:
      return chalk.yellow;
    case 5:
      return chalk.red;
    default:
      return chalk.white;
  }
};

const getTimeColor = (time) => {
  if (+time > 5000) return chalk.red;
  if (+time > 1000) return chalk.yellow;
  return chalk.white;
};

app.use(morgan((tokens, req, res) => { // eslint-disable-line
  let status = tokens.status(req, res);
  let responseTime = tokens['response-time'](req, res);

  const statusColor = getStatusColor(status);
  const timecolor = getTimeColor(responseTime);
  const method = chalk.cyan(`[${tokens.method(req, res)}]`);
  const url = chalk.bold(tokens.url(req, res));
  const contentLength = `${chalk.bold(tokens.res(req, res, 'content-length'))} bytes`;
  const remoteAddr = tokens['remote-addr'](req, res);
  const remoteUser = tokens['remote-user'](req, res) || 'N/A';
  const httpVersion = `HTTP:/${tokens['http-version'](req, res)}`;
  const date = `${tokens.date(req, res)}`;
  const methodUrl = `${method} ${url} ${httpVersion}`;

  status = statusColor(status);
  responseTime = `${timecolor(chalk.bold(responseTime))} ms`;

  return [
    remoteAddr,
    remoteUser,
    date,
    methodUrl,
    status,
    responseTime,
    contentLength,
  ].join(' | ');
}));

app.use('/api', routes);

export default app;