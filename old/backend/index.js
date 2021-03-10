const FileSync = require('lowdb/adapters/FileSync')
const staticService = require('serve-static');
const bodyParser = require('body-parser')
const adapter = new FileSync('db.json')
const config = require('./config.json');
const service = require('restana')()
const path = require('path');
const low = require('lowdb')
const db = low(adapter)
const serve = staticService(path.join(__dirname, 'static'))


service.use(bodyParser.json())
service.use(serve);
db.defaults({temperature: [], power: []}).write();

service
    .get('/lastTemperature', async (req, res) => {
        const temperatures = db.get('temperature').value() || [];
        const lastTemperature = temperatures[temperatures.length - 1];
        if (lastTemperature == null || Object.values(lastTemperature).length === 0) {
            res.send(0);
        } else {
            res.send(Object.values(lastTemperature)[0]);
        }
    })
    .get('/all', async (req, res) => {
        const temperatures = db.get('temperature').value() || [];
        const power = db.get('power').value() || [];
        res.send({temperatures, power});
    })
    .post('/temperature', async (req, res) => {
        const {val} = req.query;
        //if the sensor has a meassurement problem, we get -127c
        if (val > -100) {
            db
                .get('temperature').push({[Date.now()]: val})
                .write();
        }
        res.send();
    })
    .post('/power', async (req, res) => {
        const {val} = req.query;
        db
            .get('power').push({[Date.now()]: val})
            .write();
        res.send();
    });

service.start(config.port).then(() => {
    console.log(`Started at port ${config.port}`);
});


