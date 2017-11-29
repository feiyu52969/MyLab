$(function()
{
  /*
  //入れられた記事データをシャッフルして、ボックス状に表示する関数。
  function renderArticles(data)
  {
    var list = [];
    var $articles = $('#articles');
	  $(data).each(function()
	  {
      var article = this;

      var $box = $('<section class=".box .small"></section>');
		
      var $author = $('<p class="subheading"></p>').text('ペンネーム：').text(article.author);
      var $genre = $('<p class="subheading"></p>').text('ジャンル：').text(article.genre);
      var $title = $('<p class="subheading"></p>').text('タイトル：').text(article.title);
      var $desc = $('<p class="subheading"></p>').text('レヴュー：').text(article.desc);
        
      $box
			.append($author)
			.append($genre)
			.append($title)
			.append($desc);
 
      list[article.id] = $box
    });
    list.shuffle();
    for(var i = 0; i < list.length; i++) 
    {
      list[i].appendto($articles)
    }
  }  

  //jsonpでjson形式の記事データを取得し、それをレンダリング。
  $.ajax
  ({
    url: 'https://review-tweeter.herokuapp.com/reviews.json?callback=?',
    dataType: 'jsonp',
    crossDomain: true,
    jsonpCallback: 'getJSON'
  })
  .done(function getJSON(data)
  {
    renderArticles(data);
  })
  .fail(function()
  {
    alert('表示されるはずの記事は、読み込みできませんでした。本当は記事があります。');
  });  

  //クイックサーチ。
  $(function ()
  {
    $('input#id_search').quicksearch('p');
  });
  */

  /*ヴィデオタグ設定。
  //操作対象の#vidを指定
  var vid = document.getElementById('vid');	
  
  $.fn.videocontrol = function(options)
  {
    var target = this;

		options = $.extend(
    {
      movieWidth: targetW,
      movieHeight: targetH,
		  seekbar: false,
		  control: false,
		  chapter: false,
		  ended: false
    }, options);

		var targetW = options.movieWidth,
        targetH = options.movieHeight;
          
    function fixSize(w,h,targetWrap)
    {
      var $targetVid = $(targetWrap).children('#vid'),
				  winW	= $(window).width(),
				  winH	= $(window).height(),
				  scale	= Math.max(winW/targetW,winH/targetH),
				  fixW	= (targetW*scale)+10,
				  fixH	= (targetH*scale)+10;

			$(targetWrap).css({ width: winW, height: winH });

			$targetVid.css(
      {
				position: 'absolute',
				width: winW,
				height: winH
			});
		}

		fixSize(targetW,targetH,target);

    $(window).resize(function()
    {
			fixSize(targetW,targetH,target);
		});
  }*/

  //ボックスをグリッド表示、それと、ボックスを動かせる。
  var $boxes = $('.boxes').packery(
  {
    itemSelector: '.box',
    originLeft: false
  });
  $boxes.find('.box').each(function(i,box) 
  {
	  var draggie = new Draggabilly(box);
	  $boxes.packery('bindDraggabillyEvents',draggie);
  });

  //ループスライダー。
  new Swiper('.swiper-container',
  {
    direction: 'horizontal',
    loop: true,
    speed: 1600,
    autoplay: 2400,
    autoplayDisableOnInteraction: true,
    pagination: '.swiper-pagination',
	  paginationClickable: true,
    nextButton: '.swiper-button-next',
    prevButton: '.swiper-button-prev'
  });

  //ソーシャルボタン。
  $("#share").jsSocials(
  {
    showLabel: false,
    showCount: false,
    shares: ["twitter", "facebook", "linkedin", "pinterest"]
  });
});
  

  