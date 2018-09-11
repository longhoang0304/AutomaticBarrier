import Logs from './logs.model';

/**
 *
 * @param {Request} req
 * @param {Response} res
 * @param {Function} next
 * @param {String | ObjectId} id
 * Load single logs on queue
 */
async function load(req, res, next, logId) {
  try {
    const logs = await Logs.get(logId);
    req.logs = logs;
    return next();
  } catch (error) {
    const status = error.status || 500;
    return res.status(status).json({
      logs: error.logs,
    });
  }
}

/**
 *
 * @param {Request} req
 * @param {Response} res
 * Get an messages on queue return to client
 */
function get(req, res) {
  return res.json(req.logs);
}

/**
 *
 * @param {Request} req
 * @param {Response} res
 * Create an messages
 */
async function create(req, res) {
  const logs = new Logs({
    ...req.body,
  });
  try {
    const newMessage = await logs.save();
    return res.json(newMessage);
  } catch (error) {
    const status = error.status || 500;
    return res.status(status).json({
      logs: error.logs,
    });
  }
}

/**
 *
 * @param {Request} req
 * @param {Response} res
 * Get list all logs
 */
async function list(req, res) {
  const { limit = 50, skip = 0 } = req.query;
  try {
    const messageList = await Logs.list({ skip, limit });
    return res.json(messageList);
  } catch (error) {
    const status = error.status || 500;
    return res.status(status).json({
      logs: error.logs,
    });
  }
}

/**
 *
 * @param {Request} req
 * @param {Response} res
 * Remove an logs from list
 */
async function remove(req, res) {
  const { logs } = req;
  try {
    const deletedMessage = await logs.remove();
    return res.json(deletedMessage);
  } catch (error) {
    const status = error.status || 500;
    return res.status(status).json({
      logs: error.logs,
    });
  }
}

export { load, get, list, create, remove };