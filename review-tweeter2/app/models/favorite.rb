class Favorite < ActiveRecord::Base
  belongs_to :user
  belongs_to :review

  validates :user, presence: true
  validates :user_id, uniqueness: { scope: :review_id }
  validates :review, presence: true
end
