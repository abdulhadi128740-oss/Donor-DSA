const express = require('express');
const mongoose = require('mongoose');
const bodyParser = require('body-parser');
const session = require('express-session');
const bcrypt = require('bcryptjs');
const multer = require('multer');
const path = require('path');

// Models
const Vehicle = require('./models/Vehicle');
const User = require('./models/User');
const Service = require('./models/Service');

const app = express();

// Multer Setup for Images
const storage = multer.diskStorage({
    destination: './public/uploads/',
    filename: (req, file, cb) => {
        cb(null, 'vehicle-' + Date.now() + path.extname(file.originalname));
    }
});
const upload = multer({ storage: storage });

app.set('view engine', 'pug');
app.set('views', './views');
app.use(bodyParser.urlencoded({ extended: false }));
app.use(express.static('public'));

app.use(session({
    secret: 'vehicle-project-secret',
    resave: false,
    saveUninitialized: true
}));

// Send session data to PUG
app.use((req, res, next) => {
    res.locals.user = req.session.username;
    res.locals.isLoggedIn = req.session.isLoggedIn;
    next();
});

// Auth Guard
const isAuth = (req, res, next) => {
    if (req.session.isLoggedIn) return next();
    res.redirect('/login');
};

// DB Connection
mongoose.connect('mongodb://127.0.0.1:27017/vehicleDB')
  .then(() => console.log("✅ Database Connected"))
  .catch(err => console.log(err));

// --- ROUTES ---

// Static Pages
app.get('/about', (req, res) => res.render('about'));
app.get('/contact', (req, res) => res.render('contact'));

// Auth
app.get('/login', (req, res) => res.render('login'));
app.post('/login', async (req, res) => {
    const { username, password } = req.body;
    const user = await User.findOne({ username });
    if (user && await bcrypt.compare(password, user.password)) {
        req.session.isLoggedIn = true;
        req.session.username = username;
        res.redirect('/');
    } else {
        res.send("Invalid credentials. <a href='/login'>Try again</a>");
    }
});
app.get('/signup', (req, res) => res.render('signup'));
app.post('/signup', async (req, res) => {
    const hashed = await bcrypt.hash(req.body.password, 10);
    const newUser = new User({ username: req.body.username, password: hashed });
    await newUser.save();
    res.redirect('/login');
});
app.get('/logout', (req, res) => {
    req.session.destroy();
    res.redirect('/login');
});

// Dashboard (Main Logic)
app.get('/', isAuth, async (req, res) => {
    try {
        const stats = {
            total: await Vehicle.countDocuments(),
            cars: await Vehicle.countDocuments({ vehicleType: 'Car' }),
            bikes: await Vehicle.countDocuments({ vehicleType: 'Bike' }),
            trucks: await Vehicle.countDocuments({ vehicleType: 'Truck' })
        };
        let query = {};
        if (req.query.search) query = { regNumber: { $regex: req.query.search, $options: 'i' } };
        const vehicles = await Vehicle.find(query).sort({ createdAt: -1 });
        res.render('index', { vehicles, stats, searched: req.query.search || '' });
    } catch (err) { res.status(500).send("Error"); }
});

// Vehicle CRUD
app.get('/add', isAuth, (req, res) => res.render('add-vehicle'));
app.post('/add', isAuth, upload.single('image'), async (req, res) => {
    const data = req.body;
    if (req.file) data.image = req.file.filename;
    const v = new Vehicle(data);
    await v.save();
    res.redirect('/');
});
app.get('/edit/:id', isAuth, async (req, res) => {
    const vehicle = await Vehicle.findById(req.params.id);
    res.render('edit-vehicle', { vehicle });
});
app.post('/edit/:id', isAuth, async (req, res) => {
    await Vehicle.findByIdAndUpdate(req.params.id, req.body);
    res.redirect('/');
});
app.post('/delete/:id', isAuth, async (req, res) => {
    await Vehicle.findByIdAndDelete(req.params.id);
    res.redirect('/');
});

// Service History
app.get('/service/:id', isAuth, async (req, res) => {
    const vehicle = await Vehicle.findById(req.params.id);
    const history = await Service.find({ vehicleId: req.params.id }).sort({ date: -1 });
    const totalSpent = history.reduce((sum, r) => sum + r.cost, 0);
    res.render('service-history', { vehicle, history, totalSpent });
});
app.post('/service/add/:id', isAuth, async (req, res) => {
    const s = new Service({ ...req.body, vehicleId: req.params.id });
    await s.save();
    res.redirect('/service/' + req.params.id);
});

app.listen(3000, () => console.log("🚀 Running at http://localhost:3000"));