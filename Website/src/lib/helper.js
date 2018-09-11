import _ from 'lodash';

const APIUrl = (api) => `https://barrier-api.herokuapp.com/api/${api}`;
// const APIUrl = (api) => `http://localhost:3001/api/${api}`;

/**
 *
 * @param {String} url
 * @param {String} method
 * @param {Object} body
 * fetch API wrapper with default options
 */
const request = async (url, method, body) => {
  const headers = {
    Accept: 'application/json',
    'Content-Type': 'application/json',
  };

  const options = _.omitBy({
    method,
    headers,
    body: JSON.stringify(body),
    timeout: 15000, // 15s
  }, _.isUndefined);
  const res = await fetch(url, options);
  return res;
};

/**
 *
 * @param {String} methodName
 * generatae http method
 */
const genHttpMethod = (methodName) =>
  async (url, body) => {
    const res = await request(url, methodName.toUpperCase(), body);
    return res;
  };

const get = genHttpMethod('get');
const post = genHttpMethod('post');
const put = genHttpMethod('put');
const del = genHttpMethod('delete');

export { APIUrl, get, post, put, del };