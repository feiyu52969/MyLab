json.extract! tweet, :id, :user_id, :content, :author, :genre, :title, :created_at, :updated_at
json.url tweet_url(tweet, format: :json)
