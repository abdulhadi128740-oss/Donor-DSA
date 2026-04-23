const mongoose = require('mongoose');

const serviceSchema = new mongoose.Schema({
    vehicleId: { type: mongoose.Schema.Types.ObjectId, ref: 'Vehicle' }, // Links to the car
    serviceType: String, // Tuning, Repair, Wash, etc.
    description: String,
    cost: Number,
    date: { type: Date, default: Date.now }
});

module.exports = mongoose.model('Service', serviceSchema);