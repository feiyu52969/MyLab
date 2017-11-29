$(function()
{
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
});
  

  