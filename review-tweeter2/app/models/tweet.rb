class Tweet < ActiveRecord::Base
    belongs_to :user
    has_many :favorites, dependent: :destroy

    validates :user, presence: true
    validates :author, presence: true
    validates :genre, presence: true
    validates :title, presence: true   
    validates :content, presence: true, length:  { in: 1..140 }

    default_scope -> { order(created_at: :desc)}

    acts_as_ordered_taggable_on :genre 

    mount_uploader :image, ImageUploader

    def favorited_by? user
      favorites.where(user_id: user.id).exists?
    end
end
