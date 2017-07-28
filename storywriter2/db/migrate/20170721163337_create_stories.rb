class CreateStories < ActiveRecord::Migration[5.1]
  def change
    create_table :stories do |t|
      t.integer :user_id
      t.string :Sw
      t.string :eve
      t.string :mC
      t.string :t
      t.string :opp
      t.string :par
      t.string :nSw
      t.string :ttt
      t.string :P
      t.string :mSw
      t.string :Ce
      t.string :content

      t.timestamps

      t.index :user_id
      t.index :created_at
    end
  end
end
