class Story < ApplicationRecord
  belongs_to :user
  has_many :favorites, dependent: :destroy

  validates :user, presence: true
  validates :Sw, presence: true, length: { in: 1..500 }
  validates :eve, presence: true, length: { in: 1..500 }
  validates :mC, presence: true, length: { in: 1..500 }
  validates :t, presence: true, length: { in: 1..500 }
  validates :opp, presence: true, length: { in: 1..500 }
  validates :par, presence: true, length: { in: 1..500 }
  validates :nSw, presence: true, length: { in: 1..500 }
  validates :ttt, presence: true, length: { in: 1..500 }
  validates :P, presence: true, length: { in: 1..500 }
  validates :mSw, presence: true, length: { in: 1..500 }
  validates :Ce, presence: true, length: { in: 1..500 }
  validates :content, presence: true, length: { in: 1..1000 }

  default_scope -> { order(created_at: :desc) }

  def favorited_by? user
    favorites.where(user_id: user.id).exists?
  end
end
