var AWS = require('aws-sdk');

AWS.config.loadFromPath('./aws_config.json');

var s3 = new AWS.S3();

if (process.argv.length < 4) {
  console.log("usage: node get-s3-object.js <bucket> <key>");
  process.exit(1)
}

var params = {
  Bucket: process.argv[2],
  Key: process.argv[3]
};

s3.getObject(params, function(err, data) {
  if (err) {
    console.log(err, err.stack);
    process.exit(1);
  } else {
    console.log(data.Body.toString());
    process.exit(0);
  }
});
