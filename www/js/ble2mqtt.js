/* Management */
function reset() {
  $.ajax({
    url: '/reset',
    type: 'POST',
  });
}

/* OTA */
function otaStartUpload(type) {
  var file = $('#' + type + '-file')[0].files[0];
  if (!file)
    return;

  $('[id$=upload]').attr('disabled', true);
  $('[id$=file]').attr('disabled', true);
  $('#ota-spinner').show();
  $('#ota-status').text('Uploading');

  $.ajax({
    url: '/ota/' + type,
    type: 'POST',
    data: file,
    processData: false,
  })
  .done(onOtaUploadCompleted)
  .fail(onOtaUploadCompleted);
}

function onOtaUploadCompleted(data, textStatus, jqXHR) {
  $('#ota-status').text(textStatus);
  $('#ota-spinner').hide();
  $('[id$=file]').attr('disabled', false);
  $('[id$=upload]').attr('disabled', false);
}

/* File Manager */
function fileManagerGetFilesList() {
  path = $('#file-manager-path').text();

  $('#file-manager-list').empty();
  $.ajax({
    url: '/fs' + path,
    type: 'GET',
    processData: false,
  })
  .done(onFileManagerFilesReady);
}

function onFileManagerFilesReady(data, textStatus, jqXHR) {
  for (entry of data) {
    var div = $('<div class="file-manager-entry file-manager-entry-' + entry.type + '"></div>')
      .click(onfileManagerClick)
      .dblclick(entry.type, onfileManagerDoubleClick);
    div.append($('<span class="file-manager-name"></span').text(entry.name));
    if (entry.type === 'file')
      div.append($('<span class="file-manager-size"></span').text(entry.size));
    $('#file-manager-list').append(div);
  }
}

function onfileManagerClick(e) {
  $("#file-manager-list .highlight").removeClass('highlight');
  $(this).addClass('highlight');
}

function onfileManagerDoubleClick(e) {
  var name = $(".file-manager-name", this).text();

  if (e.data == 'file')
    fileManagerDownloadFile(name);
  else {
    $('#file-manager-path').append(name + '/');
    fileManagerGetFilesList();
  }
}

function fileManagerDownloadFile(fileName) {
  window.location.href = '/fs' + $('#file-manager-path').text() + fileName;
}

function fileManagerUp() {
  var path = $('#file-manager-path').text().split('/');
  path.splice(-2, 2);
  $('#file-manager-path').text(path.join('/') + '/');
  fileManagerGetFilesList();
}

function fileManagerDelete() {
  var name = $(".file-manager-entry.highlight .file-manager-name").text();
  if (name == '')
    return;

  $.ajax({
    url: '/fs' + $('#file-manager-path').text() + name,
    type: 'DELETE',
    processData: false,
  })
  .done(fileManagerGetFilesList);
}

function startFileManagerUpload() {
  var file = $('#file-manager-file')[0].files[0];
  if (!file)
    return;

  $('[id$=upload]').attr('disabled', true);
  $('[id$=file]').attr('disabled', true);

  $.ajax({
    url: '/fs' + $('#file-manager-path').text() + file.name,
    type: 'POST',
    data: file,
    processData: false,
  })
  .done(onFileManagerUploadCompleted)
  .fail(onFileManagerUploadCompleted);
}

function onFileManagerUploadCompleted(data, textStatus, jqXHR) {
  $('[id$=file]').attr('disabled', false);
  $('[id$=upload]').attr('disabled', false);
  fileManagerGetFilesList();
}

/* Generic */
$(document).ready(function() {
  fileManagerGetFilesList();
});
