function startUpload(type) {
  var file = $('#' + type + '-file')[0].files[0];
  if (!file)
    return;

  $('[id$=upload]').attr('disabled', true);
  $('[id$=file]').attr('disabled', true);
  $('#ota-spinner').show();
  $('#ota-status').text('Uploading');

  $.ajax({
    url: '/ota/' + type,
    type: "POST",
    data: file,
    processData: false,
  })
  .done(onUploadCompleted)
  .fail(onUploadCompleted)
}

function onUploadCompleted(data, textStatus, jqXHR) {
  $('#ota-status').text(textStatus);
  $('#ota-spinner').hide();
  $('[id$=file]').attr('disabled', false);
  $('[id$=upload]').attr('disabled', false);
}
