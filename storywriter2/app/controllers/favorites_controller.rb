class FavoritesController < ApplicationController
  before_filter :require_login

  def create
    @story   = Story.find(params[:story_id])
    @favorite = current_user.favorites.build(story: @story)

    if @favorite.save
      redirect_to request.referer, notice: "お気に入りに登録しました。"
    else
      redirect_to request.referer, alert: "このストーリーはお気に入りに登録できません。"
   end
  end

  def destroy
    @favorite = current_user.favorites.find_by!(story_id: params[:story_id])
    @favorite.destroy
    redirect_to request.referer, notice: "お気に入りを解除しました。"
  end
end
