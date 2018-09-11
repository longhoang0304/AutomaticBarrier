import express from 'express';
import * as logsCtrl from './logs.controller';


const router = express.Router();

// /**
//  *
//  * @param {String} id
//  * @param {Function} res401
//  * @param {Request} req
//  * @param {Response} res
//  * @param {Function} next
//  * callback function helps verify user
//  */
// function verificationFunction(req, res401, next, id, res) {
//   const baseUrlPattern = /^\/api\/actions\/?$/;
//   const fullPattern = /^\/api\/actions\/(.+)$/;
//   const { method, originalUrl } = req;
//   const isBase = baseUrlPattern.test(originalUrl);

//   if (isBase && method === 'POST') {
//     const { from, to } = req.body;
//     if (id !== from.senderId) {
//       return res401();
//     }
//     if (to.receiverId === from.senderId) {
//       return res401();
//     }
//     if (to.deviceId === from.deviceId) {
//       return res401();
//     }
//     return next();
//   }

//   const match = fullPattern.exec(originalUrl);
//   const receiverId = match[0];

//   if (!_.isString(receiverId) || !receiverId) {
//     return res.status(httpStatus.BAD_REQUEST).json({ message: 'Invalid receiver id' });
//   }

//   return next();
// }

// /**
//  *
//  * @param {Request} req
//  * @param {Response} res
//  * @param {Function} next
//  * Middleware helps verrify user
//  */
// const verifyUser = genUserVerification(verificationFunction);
// router.use(verifyUser);

/**
 * List all messages
 */
router.route('/')
  .get(logsCtrl.list)
  .post(logsCtrl.create);

/**
 * Get a single message
 */
router.route('/:logId')
  .get(logsCtrl.get);

/**
 * Load message when API with userId route parameter is hit
 */
router.param('logId', logsCtrl.load);

export default router;