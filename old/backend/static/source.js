window.onload = () => {

    const formatDate = (date) => {
        return new Intl.DateTimeFormat('default', {
            year: 'numeric',
            month: 'numeric',
            day: 'numeric',
            hour: 'numeric',
            minute: 'numeric',
            second: 'numeric'
        }).format(date);
    };

    const buildChart = ({containerId, title, suffix = '', data, subTitle = ''}) => {
        Highcharts.chart(containerId, {
            time: {
                useUTC: false
            },
            title: {
                text: title
            },
            subtitle: {
                text: subTitle
            },
            tooltip: {
                xDateFormat: '%Y-%m-%d | %H:%M:%S',
                shared: true,
                formatter: null,
            },
            xAxis: {
                type: 'datetime',
                crosshair: {
                    snap: false,
                },
            },
            chart: {
                type: 'line',
                zoomType: 'x',
                plotBackgroundColor: null,
                plotBorderWidth: null,
            },
            yAxis: [{
                labels: {
                    format: `{value}${suffix}`,

                },
                title: {
                    text: title,
                }
            }],
            plotOptions: {
                line: {
                    animation: false,
                    marker: {
                        enabled: false,
                    },
                },
                series: {
                    lineWidth: 1.5,
                    connectNulls: false,
                    marker: {
                        enabled: false,
                    }
                }
            },
            series: [{
                name: title,
                data,
                tooltip: {
                    valueSuffix: ` ${suffix}`
                }
            }],
            noData: {
                style: {
                    fontWeight: 'bold',
                    fontSize: '15px',
                    color: 'rgb(43, 144, 143)'
                }
            },
            responsive: {
                rules: [{
                    condition: {
                        maxWidth: 500
                    },
                    chartOptions: {
                        legend: {
                            layout: 'horizontal',
                            align: 'center',
                            verticalAlign: 'bottom'
                        }
                    }
                }]
            }
        });
    };

    (async () => {
        const data = await fetch("/all");
        const json = await data.json();

        const temperatures = json.temperatures.map(value => {
            return [parseInt(Object.keys(value)[0]), parseFloat(Object.values(value)[0])];
        });
        const power = json.power.map(value => {
            return [parseInt(Object.keys(value)[0]), parseFloat(Object.values(value)[0])];
        });

        const latestTemperature = temperatures.length > 0 ? temperatures[temperatures.length - 1] : null;
        const temperatureSubLabel = latestTemperature ? `Latest ${latestTemperature[1]} ° C (${formatDate(latestTemperature[0])})` : null;

        buildChart({
            containerId: 'temperature',
            title: 'Temperature',
            subTitle: temperatureSubLabel,
            suffix: '° C',
            data: temperatures
        });

        const latestPower = power.length > 0 ? power[power.length - 1] : null;
        const powerSubLabel = latestPower ? `Latest ${latestPower[1]} (${formatDate(latestPower[0])})` : null;

        buildChart({
            containerId: 'power',
            title: 'Power',
            subTitle: powerSubLabel,
            data: power
        });
    })();
};