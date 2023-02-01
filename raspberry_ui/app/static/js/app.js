
          
$( function() {
	  var  file_time_key = $( "#file_time" ).html();
	  file_time_key = file_time_key.trim();
	  if (file_time_key != "") {
           $("#file_time_input").val(file_time_key);
	  }
	  
	  var  ft = $( "#file_time" ).html();
	  ft = ft.trim();
	  var  msg = $( "#message_area" ).html();
	  msg = msg.replace('<div id="file_time">', "");
	  msg = msg.replace('</div>', "");
	  msg = msg.trim();
	  if (msg == "" && ft=="") {
	   	  $('#message_area').hide();
	  } else {
		  $( "#message_area" ).show();
	  }
	  var btn_status = $("#start_move_btn_status").html();
	  btn_status = btn_status.trim();
	  if (btn_status != "") {
		  $("#btn-start-move").removeAttr('disabled');
	  }

	  $("#msg-close-btn").on('click', function () {
	      $('#message_area').html("");
		  $('#message_area').hide();
	  })
	    
	  $( "#file-upload-form" ).submit(function( event ) {
  		 var file_val = $("#file").val();
		 if (file_val == "" || file_val == undefined) {
			$('#message_area').html('Empty File<button id="msg-close-btn-js" class="close-btn">&times;</button>');
			$("#message_area").show();
			//Not a good practice but need to bind the click event after client-side js add new button
			$("#msg-close-btn-js").on('click', function () {
	           $('#message_area').html("");
		       $('#message_area').hide();
	        })
			event.preventDefault();
		  } else {
		    $( "#file-upload-form" ).submit();	
		  }
  		
	   });
	  
	  
    } );
