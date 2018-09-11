import express from 'express';
import logsRoute from './logs/logs.route';

const router = express.Router(); // eslint-disable-line new-cap


/** GET /health-check - Check service health */
router.get('/health-check', (req, res) =>
  res.json({ message: 'OK' }));

// mount user routes at /users
router.use('/logs', logsRoute);


export default router;