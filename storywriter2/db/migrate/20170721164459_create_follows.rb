class CreateFollows < ActiveRecord::Migration[5.1]
  def change
    create_table :follows do |t|
      t.integer :follower_id
      t.integer :inverse_follower_id

      t.timestamps
    end
  end
end
