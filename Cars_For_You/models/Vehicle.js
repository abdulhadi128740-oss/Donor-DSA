const mongoose = require('mongoose');

const vehicleSchema = new mongoose.Schema({
    ownerName: String,
    vehicleType: String,
    regNumber: String,
    modelYear: Number,
    status: { type: String, default: 'Available' },
    image: { type: String, default: 'no-image.jpg' }, // Added this
    createdAt: { type: Date, default: Date.now }
});

module.exports = mongoose.model('Vehicle', vehicleSchema);