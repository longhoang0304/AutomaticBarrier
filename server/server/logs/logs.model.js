import mongoose, { Schema } from 'mongoose';
import httpStatus from 'http-status';
import APIError from '../helper/APIError';

const LogsScheme = new Schema(
  {
    trainCode: {
      type: String,
      required: true,
    },
    routeCode: {
      type: String,
      required: true,
    },
    station: {
      type: String,
      required: true,
    },
    speed: {
      type: Number,
      required: true,
    },
    createdAt: {
      type: Schema.Types.Date,
      required: true,
      default: new Date(),
    },
  },
  {
    collection: 'logs',
  },
);

LogsScheme.method({
});

LogsScheme.statics = {
  async get(deviceId) {
    const sort = {
      priority: -1,
      createdAt: -1,
    };
    const msg = await this.findOneAndRemove({
      to: deviceId,
    }, { sort }).exec();
    if (msg) {
      return msg;
    }
    throw new APIError('No such log exist!', httpStatus.NOT_FOUND);
  },
  list({ skip = 0, limit = 50 } = {}) {
    return this.find()
      // .sort({ createdAt: -1 })
      .skip(+skip)
      .limit(+limit)
      .exec();
  },
};

export default mongoose.model('Logs', LogsScheme);