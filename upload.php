<?php
// url to store images	
$path = '/www/crunchsnpr';
// Uniform resource identifier
// We'll access images this way
$uri = 'http://' . $_SERVER['HTTP_POST'] . '/get/';

// if file doesn't exist or has no-size:
if(!isset($_FILES['imagedata']['error']) || $_FILES['imagedata']['size'] < 1){
	echo $uri, 'error.png';
	exit;
}

// generating random unique filenames
// this way we'll avoid people gathering others uploads

$i = 0;

do{
	$filename = substr(md5(time() . $++i), -6) . '.png';
	$filepath = "$path$filename";
}

while(file_exists($filepath));

// move file

$image = @imagecreatefrompng($filepath);
imagepng($image, $filepath, 9);
imagedestroy($image);

echo $uri,$filename;

?>