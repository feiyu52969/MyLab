class Favorite < ApplicationRecord
   belongs_to :user
  belongs_to :story

  validates :user, presence: true
  validates :user_id, uniqueness: { scope: :story_id }
  validates :story, presence: true
end
